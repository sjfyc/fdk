function test()
	test2()
end

function test2()
	func()
end

test()

--[[ Desc the stack
[0]func,?:? ==> 现在在func函数内，但运行在哪个文件哪行是未知的
[1]test2,main.lua:6
[2]test,main.lua:2
--]]