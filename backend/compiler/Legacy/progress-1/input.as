$$ =============================
$$ ASTROSCRIPT FULL LEXICAL TEST
$$ =============================

$*
   Multi-line comment block
*$

mission AstroMission

launch {

    $$ Primitive Declarations

    telemetry count astronauts := 5.
    telemetry real altitude := 12000.5.
    telemetry precise fuel := 98765.4321.
    telemetry flag engine_on := 1.
    telemetry symbol grade := "A".
    telemetry voidspace nothing.

    limit count MAX_STAGE := 3.

    $$ Alias & Mode

    alias count stage_index.

    mode MissionState {
        trajectory 1.
        trajectory 2.
        fallback 0.
    }

    $$ Arithmetic Operators

    telemetry count result.
    result := 10 add 5.
    result := result minus 3.
    result := result mul 2.
    result := result divide 4.
    result := result mod 3.
    result := 2 ** 8.

    $$ Logical Operators

    verify (engine_on AND astronauts > 0) {
        coast.
    }

    verify (fuel > 1000 OR altitude < 5000) {
        stage_sep.
    }

    verify (NOT engine_on) {
        engine_on := 1.
    }

    verify (astronauts XOR 0) {
        coast.
    }

    $$ Relational Operators

    verify (fuel >= 50000) {
        altitude := altitude add 100.
    }

    verify (fuel <= 100000) {
        altitude := altitude minus 100.
    }

    verify (fuel == 98765.4321) {
        altitude := 0.
    }

    verify (fuel != 0) {
        altitude := altitude.
    }

    verify (astronauts < 10) {
        altitude := altitude.
    }

    verify (astronauts > 2) {
        altitude := altitude.
    }

    $$ Orbit Loop

    orbit (astronauts > 0) {
        astronauts := astronauts minus 1.
    }

    $$ Scenario Switch

    scenario (astronauts) {
        trajectory 0:
            altitude := 100.
        trajectory 1:
            altitude := 200.
        fallback:
            altitude := 300.
    }

    $$ Wait & Tick

    wait 5 tick.

    $$ OOP Structure

    module Rocket {

        public telemetry precise thrust.
        private telemetry count stages.

        command ignite(precise power) : voidspace {
            this.thrust := power.
            back.
        }

        command separate() : voidspace {
            stages := stages minus 1.
            back.
        }
    }

    module Falcon extends Rocket {

        public telemetry count boosters.

        command Falcon(count b) : voidspace {
            boosters := b.
            back.
        }
    }

    telemetry Rocket apollo.
    apollo := deploy Rocket().

    apollo.ignite(1000.5).
    apollo.separate().

}

success
