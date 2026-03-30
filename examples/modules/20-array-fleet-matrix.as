mission ArrayFleetMatrix launch
{
  telemetry count row.
  telemetry count col.
  telemetry count index.
  fleet count matrix[9].
  telemetry count last.

  orbit times (row : 0 : 3)
  {
    orbit times (col : 0 : 3)
    {
      index := row mul 3 add col.
      matrix[index] := row mul 10 add col.
    }
  }

  last := 0.
  orbit times (index : 0 : 9)
  {
    transmit matrix[index].
    last := matrix[index].
  }

  transmit last.
}
success
