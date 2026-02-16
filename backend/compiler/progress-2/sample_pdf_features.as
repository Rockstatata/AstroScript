mission Demo launch {
    telemetry count i.
    module M {
        command ping(count n) :-> count {
            broadcast(n).
            back.
        }
    }
    orbit while (i < 3) {
        i := i add 1.
        stage_sep.
    }
    orbit times (i:0:10) {
        alarm(i).
        coast.
    }
    telemetry real r := root(9).
} success
