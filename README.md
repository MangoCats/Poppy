# Poppy
HTTP accessible script launcher

# you will need to copy Poppy.ini to /home/user/.config/MangoCats/
# or C:\Users\<username>\AppData\Roaming\MangoCats\

# to autolaunch: (change Exec path to wherever you keep your Poppy executable)
mkdir ~/.config/autostart
nano ~/.config/autostart/Poppy.desktop
```
[Desktop Entry]
Type=Application
Name=Poppy
Exec=/home/user/Dev/Poppy/Poppy
StartupNotify=false
Terminal=true
```

Scripts added to the folder will automatically be shown on next page reload.

If you add a line:
```
#PoppyName=Display Name
```
to the script, that's what will show on the webpage.

If you add a .png file with the same name as the .sh script file, that image
will be embedded in the webpage.  Recommend resizing images down to 180 pixels
in width for quicker page rendering and loading.

.png images added to the Poppy/images folder will be available for the
/splash/time/name function just substitute the image file name (without the .png)
for name, and specify the time to show the image in seconds in the time field.
