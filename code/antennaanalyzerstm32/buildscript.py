Import("env", "projenv")

#env.Replace(
#    UPLOADER = "dfu-util",
#    UPLOADCMD = "$UPLOADER $UPLOADERFLAGS -D $SOURCE"
#)

def on_upload(source, target, env):
    firmware_path = str(source[0])
    firmware_path_bin = firmware_path[:-3] + "bin"
    # do something
    env.Execute("e:\\tools\\dfu-util -a 0 -s 0x8000000:leave -D " + firmware_path_bin)
    #firmware_path_dfu = firmware_path[:-3] + "dfu"
    # do something
    #env.Execute("e:\\tools\elf2dfuse " + firmware_path + " " + firmware_path_dfu)
    #env.Execute("e:\\tools\\dfu-util -a 0 -D " + firmware_path_dfu)

env.Replace(UPLOADCMD=on_upload)

env.Append(
    LINKFLAGS=[
#    "-mthumb",
    "-mfloat-abi=hard",
    "-mfpu=fpv4-sp-d16",
    "-u_printf_float",
 #   "-mcpu=%s" % env.BoardConfig().get("build.cpu"),
 #   "--specs=nano.specs",
 #   "--specs=nosys.specs"
])