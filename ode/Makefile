libs:
	$(MAKE) -C OPCODE
	$(MAKE) -C OPCODE/Ice
	$(MAKE) -C ode/src 
	$(MAKE) -C drawstuff/src 

install:
	$(MAKE) -C OPCODE install
	$(MAKE) -C OPCODE/Ice install
	$(MAKE) -C ode/src install
	$(MAKE) -C drawstuff/src install

test:
	$(MAKE) -C ode/test

clean:
	$(MAKE) -C OPCODE clean
	$(MAKE) -C OPCODE/Ice clean
	$(MAKE) -C ode/src clean
	$(MAKE) -C drawstuff/src clean
	$(MAKE) -C ode/test clean
