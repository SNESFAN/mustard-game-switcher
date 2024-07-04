#!/bin/sh

. /opt/muos/script/var/func.sh

pkill -STOP muxtask
/opt/muos/extra/muxlog &
sleep 1

TMP_FILE=/tmp/muxlog_global
rm -rf "$TMP_FILE"

#   Check if the backup file exists, if not then make a backup and replace the line, create installed flag
if [ ! -f /opt/muos/script/mux/frontend.sh.bak ]; then
    sed -i.bak 's#nice --20 /opt/muos/extra/muxplore -i 0 -m history#nice --20 /mnt/mmc/MUOS/application/MustardGameSwitcher.sh#' /opt/muos/script/mux/frontend.sh
    sync
echo "backup created, replaced history with gameswitcher, rebooting" >/tmp/muxlog_info
sleep 5
killall -q muxlog
rm -rf "$MUX_TEMP" /tmp/muxlog_*
    reboot
else
#   If the backup file exists, then restore the backup file, remove installed flag
    mv /opt/muos/script/mux/frontend.sh.bak /opt/muos/script/mux/frontend.sh
    sync
echo "restored backup, restoring history function, rebooting" >/tmp/muxlog_info
sleep 5
killall -q muxlog
rm -rf "$MUX_TEMP" /tmp/muxlog_*
    reboot
fi

#   frontend.sh location        
# /opt/muos/script/mux/frontend.sh
#
#   orignal line
# nice --20 /opt/muos/extra/muxplore -i 0 -m history
#
#   new line
# nice --20 /mnt/mmc/MUOS/application/MustardGameSwitcher.sh
