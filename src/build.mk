# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

installer:
	@$(MAKE) -C kiosk/installer installer

package:
	@$(MAKE) -C kiosk/installer

package-compare:
	@$(MAKE) -C kiosk/installer package-compare

stage-package:
	@$(MAKE) -C kiosk/installer stage-package

install::
	@$(MAKE) -C kiosk/installer install

clean::
	@$(MAKE) -C kiosk/installer clean

distclean::
	@$(MAKE) -C kiosk/installer distclean

source-package::
	@$(MAKE) -C kiosk/installer source-package

upload::
	@$(MAKE) -C kiosk/installer upload

source-upload::
	@$(MAKE) -C kiosk/installer source-upload

hg-bundle::
	@$(MAKE) -C kiosk/installer hg-bundle

l10n-check::
	@$(MAKE) -C kiosk/locales l10n-check

