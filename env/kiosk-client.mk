# kiosk-client.mk
#
# Automate checkout of the base firefox code and the kiosk client code.


# AIR mercurial repo for src files
HG_SB_SRC_URL = https://bitbucket.org/sbacoss/securebrowser7/src

# set this for newer branches
TARGET_REV=FIREFOX_29_0_1_RELEASE


OS_ARCH = $(shell uname -s)

MOZ_CONFIG = mozilla/.mozconfig

thisdir = $(notdir $(shell pwd))

all: checkout

first-checkout: checkout

first-checkout-prod: prodCheckOut

checkout: mercurial-checkout kiosk-checkout applypatch mozconfig 
prodCheckOut: mercurial-checkout kiosk-checkout-prodBuild applypatch mozconfig

mozilla-checkout: mercurial-checkout

mercurial-checkout:
	if [ ! -d "mozilla" ]; then \
	  if [ ! -d "../../mozilla" ]; then \
	    echo "Cloning mozilla source code from mozilla.org ..."; \
	    hg clone -r $(TARGET_REV)  http://hg.mozilla.org/releases/mozilla-release/ mozilla/; \
	  else \
            echo "Cloning from local mozilla source ..."; \
	    hg clone ../../mozilla mozilla/; \
	  fi \
	else \
	  echo "Mozilla source previously cloned. Reverting to checkout state..."; \
	  cd mozilla; hg update -C; \
	  echo "Reverting mozilla changes done.";\
	fi

kiosk-checkout-prodBuild: 
	cd mozilla; rm -fr kiosk; cp -r ./../../src kiosk;
	
kiosk-checkout:
	@if [ ! -d "mozilla/kiosk" ]; then \
#	  hg clone $(HG_SB_SRC_URL) mozilla/kiosk/; \
	  cp -r src mozilla/kiosk/; \
	fi

applypatch:
	@cd mozilla; patch -p1 < ../kiosk-core-changes.patch;

cleanpatch:
	@cd mozilla; hg update -C;

repatch: cleanpatch applypatch


mozconfig:
	echo '# SB Release Build' > $(MOZ_CONFIG);
  ifeq ($(OS_ARCH),Linux)
	echo '. $$topsrcdir/kiosk/config/mozconfig.'$(OS_ARCH) >> $(MOZ_CONFIG);
	echo  >> $(MOZ_CONFIG);
  else
    ifeq ($(OS_ARCH),Darwin)
	echo '. $$topsrcdir/kiosk/config/mozconfig.'$(OS_ARCH).universal >> $(MOZ_CONFIG);
	echo  >> $(MOZ_CONFIG);
    else
	echo '. $$topsrcdir/kiosk/config/mozconfig.WINNT' >> $(MOZ_CONFIG);
	echo  >> $(MOZ_CONFIG);
    endif
  endif
	echo '# SB Debug Build' >> $(MOZ_CONFIG);
  ifeq ($(OS_ARCH),Linux)
	echo '# . $$topsrcdir/kiosk/config/mozconfig-debug.'$(OS_ARCH) >> $(MOZ_CONFIG);
	echo  >> $(MOZ_CONFIG);
  else
    ifeq ($(OS_ARCH),Darwin)
	echo '# . $$topsrcdir/kiosk/config/mozconfig-debug.'$(OS_ARCH).universal >> $(MOZ_CONFIG);
	echo  >> $(MOZ_CONFIG);
    else
	echo '# . $$topsrcdir/kiosk/config/mozconfig-debug.WINNT' >> $(MOZ_CONFIG);
	echo  >> $(MOZ_CONFIG);
    endif
  endif
	echo '# Vanilla Firefox Release Build' >> $(MOZ_CONFIG);
  ifeq ($(OS_ARCH),Linux)
	echo '# . $$topsrcdir/browser/config/mozconfigs/linux32/release' >> $(MOZ_CONFIG);
  else
    ifeq ($(OS_ARCH),Darwin)
	echo '# . $$topsrcdir/browser/config/mozconfigs/macosx-universal/release' >> $(MOZ_CONFIG);
	echo 'mk_add_options AUTOCONF=/usr/local/Cellar/autoconf213/2.13/bin/autoconf213' >> $(MOZ_CONFIG);
	echo  'ac_add_options --with-macos-sdk=/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.9.sdk' >> $(MOZ_CONFIG);
    else
	echo '# . $$topsrcdir/browser/config/mozconfigs/win32/nightly' >> $(MOZ_CONFIG);
    endif
  endif
	echo  >> $(MOZ_CONFIG);
	echo  '# mk_add_options MOZ_OBJDIR=@TOPSRCDIR@/../opt-vanilla-@CONFIG_GUESS@' >> $(MOZ_CONFIG);
	echo  '# ac_add_options --disable-tests' >> $(MOZ_CONFIG);
	echo  >> $(MOZ_CONFIG);
	echo  '# -s makes builds quieter by default' >> $(MOZ_CONFIG);
	echo  '# -j4 allows 4 tasks to run in parallel. Set the number to be the number of' >> $(MOZ_CONFIG);
	echo  '# cores in your machine. 4 is a good number.' >> $(MOZ_CONFIG);
	echo  'mk_add_options MOZ_MAKE_FLAGS="-s -j4"' >> $(MOZ_CONFIG);
	echo  '# Turn off compiler optimization. This will make applications run slower,' >> $(MOZ_CONFIG);
	echo  '# will allow you to debug the applications under a debugger, like GDB.' >> $(MOZ_CONFIG);
	echo  '# ac_add_options --disable-optimize' >> $(MOZ_CONFIG);


hgignore:
	@echo kiosk > mozilla/.hgignore

help:
	@echo "build targets: ";\
	echo ;\
	echo "    checkout mozilla-checkout kiosk-checkout kiosk-checkout-prodBuild";\
	echo "    mozconfig applypatch repatch cleanpatch java-jep-checkout java-checkout hgignore"
	echo ;\

