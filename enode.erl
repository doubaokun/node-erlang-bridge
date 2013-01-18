-module(enode).
-export([start/0]).

listen() -> 
    receive
        [Pid, Msg] -> 
            Pid ! "hello " ++ Msg,
            %Pid ! ["hello","2"],
            %Pid ! atom,
            %Pid ! 4321,
            %Pid ! {"hello", 2, 3},
            %Pid ! {"hello", 2, [4]},
            io:format("Msg: ~p~n", [Msg])
    end,
    listen().

start() ->
    io:fwrite("Start listening for messages\n"),
    register(elogger, self()),
    listen().