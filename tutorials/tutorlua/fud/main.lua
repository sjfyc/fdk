a = array.new(10)
for i=1,10 do
	array.set(a, i, 1/i)
end

for i=1,10 do
	print(array.get(a, i))
end