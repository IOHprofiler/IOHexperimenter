import unittest
import ioh
import os
import shutil
import pickle

class Container:
    def __init__(self):
        self.xy = 0
        self.xv = 10


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
        c = Container()
        l = ioh.logger.Analyzer([ioh.logger.trigger.ALWAYS])    
        l.set_experiment_attributes({"x":"1"})
        l.watch(c, ["xv", "xy"])
        l.add_run_attributes(c, "xv")
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

    def test_pickle(self):
        try:
            pickle.dumps(ioh.logger.trigger.ON_IMPROVEMENT)
        except Exception as err: 
            self.fail(str(err))
        try:
            pickle.dumps(ioh.logger.property.RAW_Y_BEST)
        except Exception as err: 
            self.fail(str(err))
        try:
            pickle.dumps(ioh.logger.property.Property(Container(), "xy"))
        except Exception as err: 
            self.fail(str(err))

    def test_eaf(self):
        p = ioh.get_problem(1, 1, 5)
        l = ioh.logger.EAH(0, 20, 10, 0, 100, 10)
        self.assertEqual(l.error_range.min, 0)
        self.assertEqual(l.error_range.max, 20)
        self.assertEqual(l.error_range.size, 10)
        self.assertEqual(l.eval_range.min, 0)
        self.assertEqual(l.eval_range.max, 100)
        self.assertEqual(l.eval_range.size, 10)
        p.attach_logger(l)

        for i in range(10):
            p([i] * 5)

        self.assertEqual(list(l.data.keys()), [1])
                    
                    
if __name__ == "__main__":
    unittest.main()