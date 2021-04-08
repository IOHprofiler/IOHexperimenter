# import unittest
# import ioh

# class TestLogger(unittest.TestCase):
#     def test_initiate_logger(self):
#         log = ioh.logger.Default("temp/")
#         f = ioh.problem.OneMax(1,16)
#         log.track_problem(f)
#         f.evaluate(16*[0])
#         log.log(f.logger_info())
#         f.evaluate(16*[1])
#         log.log(f.logger_info())
#         log2 = ioh.BBOB_Logger("temp2/")
        

# if __name__ == "__main__":
#     unittest.main()