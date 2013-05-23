a = array.new(10)
for i=1,10 do
	array.set(a, i, 1/i)
end

for i=1,10 do
	print(array.get(a, i))
end

local mt = getmetatable(a)        --此步骤在C中完成更简单,详见fud3
mt.__index = array

print("a[10] = " .. a:get(10))
a:set(1, 2)
print("a[1] = " .. a:get(1))

-- a是User Datum, 所有对a的调用都将转向对metatable.__index的调用.
-- a:get(10) == a.get(a, 10) == a.metatable.__index.get(a, 10) == mt.__index.get(a, 10) == mt.get(a,10) == array.get(a, 10)
