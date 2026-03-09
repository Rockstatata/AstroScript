mission Demo launch {
    telemetry count x.
    telemetry count arr[3].
    x := 2 add 3 .
    verify (x > 0) {
        transmit x.
    } otherwise {
        abort.
    }
    orbit (x < 10) {
        x := x add 1.
        transmit x.
      
    }
    transmit x.
} success
