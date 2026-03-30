mission LoopsFlowControl launch
{
  telemetry count i := 0.

  orbit (i < 6)
  {
    i := i add 1.
    verify (i == 2)
    {
      coast.
    }

    transmit i.

    verify (i == 4)
    {
      stage_sep.
    }
  }

  orbit while (i < 7)
  {
    transmit i.
    i := i add 1.
  }

  orbit times (i : 0 : 3)
  {
    transmit i.
  }
}
success
