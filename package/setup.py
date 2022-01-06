from setuptools import setup, Extension
import numpy

base = Extension('deliverysimulator._base',
                 sources=["deliverysimulator/base/src/main.c",
                          "deliverysimulator/base/src/engine/engine.c",
                          "deliverysimulator/base/src/engine/event/event.c",
                          "deliverysimulator/base/src/engine/event/handler/handler_delivery.c",
                          "deliverysimulator/base/src/engine/event/handler/handler_demand.c",
                          "deliverysimulator/base/src/engine/event/handler/handler_movement.c",
                          "deliverysimulator/base/src/engine/event/handler/handler_production.c",
                          "deliverysimulator/base/src/engine/event/handler/handler_rtb.c",
                          "deliverysimulator/base/src/model/model.c",
                          "deliverysimulator/base/src/util/errcode.c",
                          "deliverysimulator/base/src/util/data_structure/graph.c",
                          "deliverysimulator/base/src/util/data_structure/list.c",
                          "deliverysimulator/base/src/util/data_structure/priority_queue.c",
                          "deliverysimulator/base/src/util/generator/generator.c",
                          "deliverysimulator/base/src/util/generator/mt19937.c"])

setup(name="deliverysimulator",
      version="0.1",
      packages=["deliverysimulator"],
      ext_modules=[base])
