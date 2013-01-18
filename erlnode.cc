#include <v8.h>
#include <node.h>

extern "C" {

#include <ei.h>
#include <erl_interface.h>

extern const char *erl_thisnodename(void);
extern short erl_thiscreation(void);
#define SELF(fd) erl_mk_pid(erl_thisnodename(), fd, 0, erl_thiscreation())

}

using namespace node;
using namespace v8;

class ErlNode: ObjectWrap {
  private:

    int m_count;
    int sockfd;
    ETERM* self;

  public:

    ErlNode() : m_count(0){}
    ~ErlNode(){}

    static Persistent<FunctionTemplate> s_erlnode;

    static void Init(Handle<Object> target) {
      HandleScope scope;

      Local<FunctionTemplate> t = FunctionTemplate::New(New);

      s_erlnode = Persistent<FunctionTemplate>::New(t);
      s_erlnode->InstanceTemplate()->SetInternalFieldCount(1);
      s_erlnode->SetClassName(String::NewSymbol("ErlNode"));

      NODE_SET_PROTOTYPE_METHOD(s_erlnode, "send_by_name", SendByName);
      NODE_SET_PROTOTYPE_METHOD(s_erlnode, "receive", Receive);
      NODE_SET_PROTOTYPE_METHOD(s_erlnode, "count", Count);
      NODE_SET_PROTOTYPE_METHOD(s_erlnode, "args", Args);
      NODE_SET_PROTOTYPE_METHOD(s_erlnode, "info", Info);

      target->Set(String::NewSymbol("ErlNode"), s_erlnode->GetFunction());
    }

    static Handle<Value> New(const Arguments& args) {
      HandleScope scope;
      ErlNode* hw = new ErlNode();

      String::AsciiValue nodename(args[0]);
      String::AsciiValue cookie(args[1]);

      erl_init(NULL, 0);
      //char *cookie = "secret";
      //char *nodename = "e1@localhost";
      if(erl_connect_init(1, *cookie, 1) == -1) {
        erl_err_quit("ERROR: erl_connect_init failed");
      }
      
      int sockfd = -1;
      if((sockfd = erl_connect(*nodename)) == -1) {
        erl_err_quit("ERROR: erl_connect failed");
      }

      hw->sockfd = sockfd;
      hw->self = SELF(sockfd);

      hw->Wrap(args.This());
      return args.This();
    }

    static Handle<Value> Receive(const Arguments& args) {
      HandleScope scope;
      ErlNode* hw = ObjectWrap::Unwrap<ErlNode>(args.This());
      int sockfd = hw->sockfd;

      // Receive msg
      int BUFSIZE = 128;
      unsigned char buf[BUFSIZE];

      ErlMessage emsg;
      ETERM *msg;

      int status;

      status = erl_receive_msg(sockfd, buf, BUFSIZE, &emsg);

      if(status == -1) {
        return scope.Close(Undefined());
      }

      while (status == ERL_TICK) {
        status = erl_receive_msg(sockfd, buf, BUFSIZE, &emsg);
      }

      msg = emsg.msg;

      if(ERL_IS_LIST(msg)) {
        char* pp;
        pp = erl_iolist_to_string(msg);

        erl_free_term(emsg.msg);
        erl_free_term(emsg.from);
        erl_free_term(emsg.to);

        Local<String> result = String::New(pp);
        return scope.Close(result);
      } else {
        char* pp;
        pp = erl_iolist_to_string(msg);

        erl_free_term(emsg.msg);
        erl_free_term(emsg.from);
        erl_free_term(emsg.to);

        Local<String> result = String::New(pp);
        return scope.Close(result);
      }
    }

    static Handle<Value> SendByName(const Arguments& args) {
      HandleScope scope;
      ErlNode* hw = ObjectWrap::Unwrap<ErlNode>(args.This());
      hw->m_count++;

      int sockfd = hw->sockfd;
      ETERM *self = hw->self;

      String::AsciiValue pname(args[0]);
      String::AsciiValue eformat(args[1]);
      //String::AsciiValue _self(args[2]);

      String::AsciiValue command(args[3]);

      ETERM *ep;
      ep = erl_format(*eformat, self, *command);
      erl_reg_send(sockfd, *pname, ep);
      erl_free_term(ep);
        
      Local<Integer> result = Integer::New(1);
      return scope.Close(result);
    }

    static Handle<Value> Count(const Arguments& args) {
      HandleScope scope;
      ErlNode* hw = ObjectWrap::Unwrap<ErlNode>(args.This());
      Local<Integer> result = Integer::New(hw->m_count);
      return scope.Close(result);
    }

    static Handle<Value> Info(const Arguments& args) {
      HandleScope scope;
      ErlNode* hw = ObjectWrap::Unwrap<ErlNode>(args.This());

      Local<Object> info = Object::New();
      Local<String> node_name = String::New(ERL_PID_NODE(hw->self));
      info->Set(String::NewSymbol("node_name"), node_name);

      //Local<String> result = String::New(ERL_PID_NODE(hw->self));
      //Local<String> result = String::New(erl_thisnodename());
      return scope.Close(info);
    }

    static Handle<Value> Args(const Arguments& args) {
      HandleScope scope;
      //ErlNode* hw = ObjectWrap::Unwrap<ErlNode>(args.This());
      Local<String> result = args[0]->ToString();
      return scope.Close(result);
    }
};

Persistent<FunctionTemplate> ErlNode::s_erlnode;

static void init (Handle<Object> target) {
  ErlNode::Init(target);
}

NODE_MODULE(erlnode, init);