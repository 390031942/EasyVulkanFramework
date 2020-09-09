import EasyVulkan
component = EasyVulkan.component

class testClass1(component):
    def __init__(self):
        self.var1=1
        self.var2='wo aini'

    def Start(self):
        print("testClass1 Start Called")

    def Update(self):
        print('testClass1 update called')