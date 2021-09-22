import unittest
import ioh
import os
import shutil


class TestLogger(unittest.TestCase):
    def test_flatfile(self):
        pr = ioh.get_problem("Sphere", 1, 5) 
        p = [ioh.logger.property.CURRENT_Y_BEST]
        t = [ioh.logger.trigger.ON_IMPROVEMENT]
        l = ioh.logger.FlatFile(t, p, separator=" ")
        pr.attach_logger(l)
        self.assertEqual(os.path.realpath("./"), os.path.realpath(l.output_directory))
        pr([0] * 5)
        self.assertTrue(os.path.isfile("IOH.dat"))

    def test_analyzer(self):
        pr = ioh.get_problem("Sphere", 1, 5) 
        
        class Container:
            def __init__(self):
                self.xy = 0
                self.xv = 10

        c = Container()
        l = ioh.logger.Analyzer([ioh.logger.trigger.ALWAYS])    
        l.set_experiment_attributes({"x":"1"})
        l.watch(c, ["xv", "xy"])
        l.add_run_attribute(c, "xv")
        pr.attach_logger(l)

        for c.xv in range(4):
            for i in range(5):
                c.xy = i*10
                pr([i] * 5)
            pr.reset()

        self.assertTrue(os.path.isdir("ioh_data"))

    def tearDown(self) -> None:
        if os.path.isdir("ioh_data"):
            shutil.rmtree("ioh_data")
        if os.path.isfile("IOH.dat"):
            os.remove("IOH.dat")
        return super().tearDown()


        
                    

if __name__ == "__main__":
    unittest.main()