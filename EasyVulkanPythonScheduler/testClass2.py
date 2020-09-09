import EasyVulkan
Component = EasyVulkan.Component

class testClass2(Component):
    def __init__(self):
        self.var1=123.4
        self.var2=list(1,2,3)

    def Start(self):
        print("testClass2 Start Called")

    def Update(self):
        print('testClass2 update called')