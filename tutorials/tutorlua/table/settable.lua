function printcolor(c)
  io.write(string.format("%g,%g,%g", c[1],c[2],c[3]))
  io.write("\n")
end

printcolor(color.BLACK)
printcolor(color.WHITE)
printcolor(color.RED)
printcolor(color.GREEN)
printcolor(color.BLUE)