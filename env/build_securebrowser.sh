#!/bin/bash

# Take the first command-line parameter (which should be the URL of the Student site),
# Base64-encodes it, and places it in the proper configuration file.
#

# Constants
BRANDING_FILE="../src/branding/SBACSecureBrowser/pref/kiosk-branding.js";
URL_PARAM="bmakiosk.startup.homepage";

if [[ $# > 0 ]]; then
    URL=$1
    # need to validate URL as being properly formatted
    echo "URL: " $URL
    regex='(https?)://[-A-Za-z0-9\+&@#/%?=~_|!:,.;]*[-A-Za-z0-9\+&@#/%=~_|]'
    if [[ $URL =~ $regex ]] ; then # valid URL format
	enc_url=`echo $URL | base64`
	echo "Encoded URL: " $enc_url
	echo "OLD value: "
	grep $URL_PARAM $BRANDING_FILE
	sed -i .bak "s/$URL_PARAM\", *\".*\"/$URL_PARAM\",\"$enc_url\"/" $BRANDING_FILE
	echo "* DONE. New value:"
	grep $URL_PARAM $BRANDING_FILE
	shift
        ./automate.sh $@
    else
	echo "Invalid URL."
	exit 1
    fi
else
    echo
    echo "WARNING!! No URL provided. To use a generic default URL for Secure Browser home page, run ./automate.sh"
    echo "Usage: $0 homepage-url [-b branding-info]"
    echo "       homepage-url   The URL which the secure browser will open when started. If none is provided, a default test URL will be used (not recommended)."
    echo "    -b branding-info  OPTIONAL: These parameters will be passed directly into the automate.sh script"
    echo
fi


