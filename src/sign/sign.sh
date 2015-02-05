#!/bin/sh

if [ ! $1 ]; then
  echo "    Useage: $0 <file.dmg>";
  exit 1;
fi

appname=`echo $1 | cut -f1 -d-`;
name=`echo $1 | sed -e 's:.dmg::g'`;

dmgname=$name-signed.dmg;

rm -rf $appname.app;

hdiutil mount $1;

cp -RH /volumes/$appname .;

wait;

hdiutil unmount /Volumes/$appname;

wait;

mv $appname /Volumes;

echo "Signing package [$appname.app]";

codesign -s "American Institutes for Research in the Behavioral Sciences"  --resource-rules CodeResources /Volumes/$appname/$appname.app

codesign -v /Volumes/$appname/$appname.app;

if [ $? -eq 0 ]; then
  echo "Package [$appname.app] successfully signed...";
fi

echo "Creating dmg file[$dmgname]...";

hdiutil create $dmgname -srcfolder /Volumes/$appname -ov;

wait;

rm -rf /Volumes/$appname;

hdiutil attach -owners on $dmgname -shadow;

wait;

SetFile -a C /Volumes/$appname/;

hdiutil detach /Volumes/$appname;

wait;

hdiutil convert -format UDZO -o .$dmgname $dmgname -shadow;

wait;

mv .$dmgname $dmgname;

rm -f *.shadow;

echo "Signing dmg...";

codesign -s "American Institutes for Research in the Behavioral Sciences" $dmgname

codesign -v $dmgname;

if [ $? -eq 0 ]; then
  echo "dmg [$dmgname] successfully signed...";
fi

exit 0;

