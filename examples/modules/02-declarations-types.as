mission DeclarationsTypes launch
{
  alias count IntAlias.
  telemetry count c := 7.
  telemetry real r := 3.5.
  telemetry precise p := 9.25.
  telemetry flag ok := 1.
  telemetry symbol msg := "types".
  telemetry IntAlias custom := 11.
  limit count MAX_LIMIT := 100.

  fleet count dynFleet[4].
  telemetry count arr[3].
  dynFleet[0] := c add 1.
  dynFleet[1] := dynFleet[0] add 2.
  arr[0] := c.
  arr[1] := custom.
  arr[2] := MAX_LIMIT.

  transmit c.
  transmit r.
  transmit p.
  transmit ok.
  transmit msg.
  transmit custom.
  transmit dynFleet[1].
  transmit arr[1].
}
success
