#!/bin/sh
../unxbin/lang2po -i ../rocview/res/messages.xml -o rocrail.pot -template -l en
msgfmt -c rocrail.pot

