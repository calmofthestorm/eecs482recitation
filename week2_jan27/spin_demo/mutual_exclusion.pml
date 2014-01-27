bool active_flag[2];
int turn;

int ncrit;

active [1] proctype thread0()
{
forever:
  active_flag[0] = 1;
  wait:
    if
      :: (turn == 0) -> goto done_waiting;
      :: (turn != 0) -> skip;
    fi;
    (active_flag[1] == 0); // while (active_flag);
    turn = 0;
    goto wait;
  done_waiting:
    // critical section
    ncrit++;
    assert(ncrit == 1);
    ncrit--;
    active_flag[0] = 0;

    // do other stuff
    goto forever;
}

active [1] proctype thread1()
{
forever:
  active_flag[1] = 1;
  wait:
    if
      :: (turn == 1) -> goto done_waiting;
      :: (turn != 1) -> skip;
    fi;
    (active_flag[0] == 0); // while (active_flag);
    turn = 1;
    goto wait;
  done_waiting:
    // critical section
    ncrit++;
    assert(ncrit == 1);
    ncrit--;
    active_flag[1] = 0;

    // do other stuff
    goto forever;
}
