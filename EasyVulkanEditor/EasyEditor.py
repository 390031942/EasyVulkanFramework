import EasyVulkanEditor

class EasyEditorClass:
    global editorKernal
    def __init__(self):
        pass
    def quit(self):
        editorKernal.quit_Internal()
    def alert(self,msg):
        editorKernal.alert_Internal(msg)

Editor = EasyEditorClass()