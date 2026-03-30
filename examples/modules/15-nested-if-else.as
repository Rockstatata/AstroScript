mission NestedIfElse launch
{
  telemetry count fuel := 7.
  telemetry count signal := 3.

  verify (fuel > 0)
  {
    verify (signal > 5)
    {
      transmit "High fuel, high signal".
    }
    else_verify (signal > 2)
    {
      verify (fuel > 5)
      {
        transmit "High fuel, medium signal".
      }
      otherwise
      {
        transmit "Low fuel, medium signal".
      }
    }
    otherwise
    {
      transmit "High fuel, low signal".
    }
  }
  else_verify (fuel == 0)
  {
    transmit "No fuel".
  }
  otherwise
  {
    transmit "Invalid fuel".
  }
}
success
