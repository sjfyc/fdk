function test()
	test2()
end

function test2()
	func()
end

test()

--[[ Desc the stack
[0]func,?:? ==> ������func�����ڣ����������ĸ��ļ�������δ֪��
[1]test2,main.lua:6
[2]test,main.lua:2
--]]