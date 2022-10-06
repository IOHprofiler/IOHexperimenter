import os
import random
import unittest
import shutil

import ioh

class Algorithm:
    def __init__(self):
        self.x = 10
        self.i = 1
    
    def __call__(self, p: ioh.problem.Real):
        for i in range(10000):
            x = list(
                map(lambda x: 10 * random.random(), 
                range(p.meta_data.n_variables)
                ))    
            p(x)
            self.i = i


class TestExperiment(unittest.TestCase):

    def tearDown(self) -> None:
        shutil.rmtree("ioh_data")
        os.remove("ioh_data.zip")


    def test_experimenter(self):
        exp = ioh.Experiment(
            Algorithm(),
            [1], [1, 2], [5],
            njobs = 1,
            reps = 2,
            algorithm_name = "RandomSearch", 
            experiment_attributes = {"a": "1"},
            run_attributes = ['x'],
            logged_attributes = ['i'],
            logger_triggers = [
                ioh.logger.trigger.ALWAYS
            ],
            logger_additional_properties = [
                ioh.logger.property.VIOLATION,
                ioh.logger.property.PENALTY
            ],
            enforce_bounds=True,
            store_positions=True,
        )

        def a_problem(x):
            return 0.0
            
        exp.add_custom_problem(a_problem, "Name")
        pid = ioh.get_problem_id("Name", "Real")
        exp()

        info_files = {f'IOHprofiler_f{pid}_Name.info', 'IOHprofiler_f1_Sphere.info'}
        data_files = {f'IOHprofiler_f{pid}_DIM5.dat', 'IOHprofiler_f1_DIM5.dat'}

        for item in os.listdir('ioh_data'):
            path = os.path.join('ioh_data', item)
            if os.path.isfile(path) and item in info_files:
                self.assertNotEqual(os.path.getsize(path), 0)
                info_files.remove(item)
            elif os.path.isdir(path):
                for f in os.listdir(path):
                    if f in data_files:
                        path = os.path.join(path, f)
                        self.assertNotEqual(os.path.getsize(path), 0)
                        with open(path) as h:
                            data = list(filter(lambda x:x.startswith('"'), h))       

                        self.assertEqual(len(data), 4)
                        data_files.remove(f)

        self.assertSetEqual(info_files, set())
        self.assertSetEqual(data_files, set())
        self.assertTrue(os.path.isfile("ioh_data.zip"))    

    def test_experimenter_v2(self):
        ioh.Experiment(
            Algorithm(),
            [1], [1, 2], [5],
            njobs = 1,
            reps = 2,
            algorithm_name = "RandomSearch", 
            old_logger=False,
            logger_triggers = [
                ioh.logger.trigger.ALWAYS
            ],
            logger_additional_properties = [
                ioh.logger.property.VIOLATION,
                ioh.logger.property.PENALTY
            ],
            enforce_bounds=True,
        )()

        info_files = {'IOHprofiler_f1_Sphere.json'}
        data_files = {'IOHprofiler_f1_DIM5.dat'}

        for item in os.listdir('ioh_data'):
            path = os.path.join('ioh_data', item)
            if os.path.isfile(path) and item in info_files:
                self.assertNotEqual(os.path.getsize(path), 0)
                info_files.remove(item)
            elif os.path.isdir(path):
                for f in os.listdir(path):
                    if f in data_files:
                        path = os.path.join(path, f)
                        self.assertNotEqual(os.path.getsize(path), 0)
                        with open(path) as h:
                            data = list(filter(lambda x:x.startswith('evaluations'), h))       

                        self.assertEqual(len(data), 4)
                        data_files.remove(f)

        self.assertSetEqual(info_files, set())
        self.assertSetEqual(data_files, set())
        self.assertTrue(os.path.isfile("ioh_data.zip"))    
           
    


if __name__ == "__main__":
    unittest.main()
