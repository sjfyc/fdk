function test()
	test2()
end

function test2()
	func()
end

-- test() call from c++

--[[ Desc the stack
[0]func [C]:-1 ==> ����C��func�����ڣ�������C�������Բ�֪���������ĸ��ļ�������
[1]test2 main.lua:6
[2](null) main.lua:2 ==> ��c++�����lua��������������δ֪��
--]]