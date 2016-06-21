CON
    _clkmode = xtal1 + pll16x
    _xinfreq = 5_000_000

    %000_11010100
    %11010100_

OBJ
    ser : "com.serial"

PUB Main

    ser.Start(115_200)

    repeat
        ser.Char(ser.CharIn, 34, chicken)
