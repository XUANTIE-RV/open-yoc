CPRE := @
ifeq ($(V),1)
CPRE :=
VERB := --verbose
endif

cpunum = $(shell cat /proc/cpuinfo| grep "processor"| wc -l)

SDK := $(shell cat .defsdk 2> /dev/null)

.PHONY:startup
startup: all

all:
	@echo "Build Solution by $(BOARD) $(SDK) "
	@[ ! -z $(SDK) ] && echo $(SDK) > .defsdk || true
	$(CPRE) touch app/src/app_main.c
	$(CPRE) scons $(VERB) --board=$(BOARD) --sdk=$(SDK) -j$(cpunum)
	@echo YoC SDK Done

.PHONY:flashall
flashall:
	$(CPRE) scons --flash=all --board=$(BOARD) --sdk=$(SDK)

.PHONY:erasechip
erasechip:
	$(CPRE) scons --flash=erasechip --board=$(BOARD) --sdk=$(SDK)

.PHONY:flash
flash:
	$(CPRE) scons --flash=prim --board=$(BOARD) --sdk=$(SDK)

.PHONY:flashfs
flashfs:
	$(CPRE) scons --flash=lfs --board=$(BOARD) --sdk=$(SDK)

.PHONY:flashboot
flashboot:
	$(CPRE) scons --flash=boot --board=$(BOARD) --sdk=$(SDK)

.PHONY:sdklist
sdklist:
	@cat package.yaml | awk -F'[ :]' -v done=0 '{if($$0=="depends:"){exit} else if($$0=="sdk_chip:"){done=1}; if(done && substr($$0,0,3)=="  -"){print "make SDK="$$4}}'
	@echo
	@cat package.yaml | awk -F'[ :]' -v done=0 '{if(done==1 && substr($$0,0,3)!="  -"){exit} else if($$0=="depends:"){done=1} else if(done && substr($$4,0,8)=="sdk_chip"){print "Yaml Default SDK:"$$4}}'
	@echo
	@[ -f .defsdk ] && echo -n "Build Default SDK:" && cat .defsdk || true

.PHONY:targetip
targetip:
	@uname -a | grep WSL2 >/dev/null && rmtip=`route | grep ^[1-9] | awk -F'.' '{printf("%s.%s.%s.1\n",$$1,$$2,$$3)}'`;cat gdbinitflash  | awk -v ip="$$rmtip" '{if($$1=="target"){print "target remote "ip":1025"} else {print $$0}}'> .gdbtmp
	@cp .gdbtmp gdbinitflash;rm .gdbtmp
	cat gdbinitflash

.PHONY:clean
clean:
	$(CPRE) rm -rf yoc_sdk binary out yoc.*  generated
	$(CPRE) rm -fr gdbinitflash .gdbinit gdbinit


