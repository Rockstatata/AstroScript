mission FunctionsReturns launch
{
  command add_kw(count a, count b) : count
  {
    back a add b.
  }

  command mul_sym(count a, count b) :-> count
  {
    back a * b.
  }

  command ping() : voidspace
  {
    transmit "ping".
    back.
  }

  telemetry count r1 := add_kw(7, 8).
  telemetry count r2 := mul_sym(3, 4).
  transmit r1.
  transmit r2.
  ping().
}
success
