mission WaitMath launch
{
  telemetry count x := 9.
  telemetry count y.

  y := root(x).
  transmit y.
  y := flr(3.9).
  transmit y.
  y := ceil(3.1).
  transmit y.
  y := abs(-22).
  transmit y.
  y := logarithm(1).
  transmit y.
  y := sine(0).
  transmit y.
  y := cosine(0).
  transmit y.
  y := tan(0).
  transmit y.
  y := asine(0).
  transmit y.
  y := acosine(1).
  transmit y.
  y := atan(1).
  transmit y.
  y := prime(17).
  transmit y.

  wait 1 tick.
}
success
