mission Demo launch {
    telemetry count x.
    telemetry count arr[3].
    x := 2 add 3 mul 4.
    verify (x > 0) {
        wait 1 tick.
    } otherwise {
        abort.
    }
    orbit (x < 10) {
        x := x add 1.
        coast.
    }
} success
