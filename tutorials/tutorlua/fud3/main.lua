a = array.new(10)
for i=1,10 do
	a:set(i, 1/i)
end

for i=1,10 do
	print(a:get(i))
end