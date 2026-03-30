mission NestedFunctionCalls launch
{
  command boost(count base) : count
  {
    back base add 2.
  }

  command triple(count base) : count
  {
    back base mul 3.
  }

  command nested_compute(count seed) : count
  {
    telemetry count staged := triple(boost(seed)).
    back boost(staged).
  }

  telemetry count result := nested_compute(boost(4)).
  transmit result.
}
success
