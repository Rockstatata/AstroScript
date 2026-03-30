mission ControlFlow launch
{
  telemetry count fuel := 5.

  verify (fuel > 3)
  {
    transmit "Launch condition satisfied".
  }
  else_verify (fuel == 3)
  {
    transmit "Edge condition".
  }
  otherwise
  {
    transmit "Fuel below launch threshold".
  }
}
success
