import unittest
import ioh
import os
import shutil
import pickle
import json


class Container:
    def __init__(self):
        self.xy = 0
        self.xv = 10


class TestLogger(unittest.TestCase):
    def test_flatfile(self):
        pr = ioh.get_problem("Sphere", 1, 5) 
        p = [ioh.logger.property.CURRENTBESTY]
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
        l.add_run_attribute(c, "xv")
        pr.attach_logger(l)

        for c.xv in range(4):
            for i in range(5):
                c.xy = i*10
                pr([i] * 5)
            pr.reset()
        l.close()
        
        self.assertTrue(os.path.isdir("ioh_data"))
        meta = os.path.join("ioh_data", "IOHprofiler_f1_Sphere.json")
        data = os.path.join("ioh_data", "data_f1_Sphere", "IOHprofiler_f1_DIM5.dat")
        self.assertTrue(os.path.isfile(meta))
        self.assertTrue(os.path.isfile(data))

        with open(meta) as f:
            meta_data = json.loads(f.read())
            for i, run in enumerate(meta_data["scenarios"][0]["runs"]):
                self.assertEqual(run['xv'], i)
        with open(data) as f:
            next(f)
            for i in range(5):
                self.assertEqual(i*10, float(next(f).split()[-1]))

    def test_triggers(self):
        always = ioh.logger.trigger.Always()
        on_improvement = ioh.logger.trigger.OnImprovement()
        at = ioh.logger.trigger.At({1, 22})
        self.assertSetEqual(at.time_points, {1, 22})

        each = ioh.logger.trigger.Each(10, 2)
        self.assertEqual(each.interval, 10)
        self.assertEqual(each.starting_at, 2)

        during = ioh.logger.trigger.During({(10, 25), (30, 40)})
        self.assertEqual(during.time_ranges, {(10, 25), (30, 40)})

        l = ioh.LogInfo(22, 1, 1, 1, 1, 1, 1, [0, 1],[0, 1],[0, 1], ioh.RealSolution([0, 1], 0), True)
        problem = ioh.get_problem(1, 1, 2)
        for t in (always, on_improvement, at, each, during):
            self.assertTrue(t(l, problem.meta_data))


    def test_multiple_problems_in_sequence(self):
        p1 = ioh.get_problem(1, 2, 5)
        p2 = ioh.get_problem(2, 2, 5)
        l = ioh.logger.Analyzer() 
        for i in range(5):
            for p in p1, p2:
                p.attach_logger(l)
                for _ in range(10):
                    p([0] * 5)
                p.reset()
        
        for p in p1, p2:
            i = p.meta_data.problem_id
            name = p.meta_data.name
            meta = os.path.realpath(os.path.join("ioh_data", f"IOHprofiler_f{i}_{name}.json"))
            data = os.path.realpath(os.path.join("ioh_data", f"data_f{i}_{name}", f"IOHprofiler_f{i}_DIM5.dat"))
            self.assertTrue(os.path.isfile(meta))
            self.assertTrue(os.path.isfile(data))
            with open(meta) as f:
                meta_data = json.load(f)
                self.assertEqual(len(meta_data["scenarios"][0]["runs"]), 5)
            with open(data) as f:
                n_headers = 0
                for line in f:
                    if line.startswith("evaluations"):
                        n_headers += 1
                self.assertEqual(n_headers, 5)


    def test_multiple_problems_has_info_file(self):
        p1 = ioh.get_problem(1, 2, 5)
        p2 = ioh.get_problem(2, 2, 5)
        l = ioh.logger.Analyzer() 


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
            pickle.dumps(ioh.logger.property.RAWYBEST)
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
                    
    def test_can_inherit_property(self):
        class P(ioh.logger.property.AbstractProperty):
            def __call__(self, _):
                return 10.0
        c = ioh.RealSolution([0], 1)
        l = ioh.LogInfo(0, 1, 1, 1, 1, 1, 1, [0],[0],[0], c)
        a = P("x")
        self.assertEqual(int(a(l)), int(float(a.call_to_string(l, "na"))))
        

if __name__ == "__main__":
    unittest.main()