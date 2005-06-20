libs:
	make -C OPCODE
	make -C OPCODE/Ice
	make -C ode/src 
	make -C drawstuff/src 

install:
	make -C OPCODE install
	make -C OPCODE/Ice install
	make -C ode/src install
	make -C drawstuff/src install

test:
	make -C ode/test

clean:
	make -C OPCODE clean
	make -C OPCODE/Ice clean
	make -C ode/src clean
	make -C drawstuff/src clean
	make -C ode/test clean
