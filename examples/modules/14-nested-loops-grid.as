mission NestedLoopsGrid launch
{
  telemetry count row.
  telemetry count col.
  telemetry count total := 0.

  orbit times (row : 0 : 3)
  {
    orbit times (col : 0 : 3)
    {
      telemetry count cell := row mul 10 add col.
      transmit cell.
      total := total add cell.
    }
  }

  transmit total.
}
success
