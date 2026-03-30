mission NestedControlFlowMixed launch
{
  telemetry count outer := 0.
  telemetry count inner.

  orbit (outer < 3)
  {
    inner := 0.

    orbit (inner < 4)
    {
      verify ((outer add inner) mod 2 == 0)
      {
        transmit outer add inner.
      }
      otherwise
      {
        verify (inner == 3)
        {
          transmit outer mul 100 add inner.
        }
        otherwise
        {
          inner := inner add 1.
          coast.
        }
      }

      inner := inner add 1.
    }

    outer := outer add 1.
  }
}
success
