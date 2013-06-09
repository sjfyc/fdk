function test()
	test2()
end

function test2()
	func()
end

-- test() call from c++

--[[ Desc the stack
[0]func [C]:-1 ==> 现在C的func函数内，由于是C函数所以不知道具体在哪个文件的哪行
[1]test2 main.lua:6
[2](null) main.lua:2 ==> 从c++发起的lua函数调用名称是未知的
--]]