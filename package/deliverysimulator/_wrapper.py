import deliverysimulator._base

class Result:
    def __init__(self, profit, num_orders, num_delivered):
        self.profit = profit,
        self.num_orders = num_orders
        self.num_delivered = num_delivered
    def to_dict(self):
        return {
            "profit": self.profit,
            "num_orders": self.num_orders,
            "num_delivered": self.num_delivered
        }

def run(time_end: float,
        seed: int,
        num_vehicles: int,
        vehicle_cost: float,
        unit_cost: list,
        unit_revenue: list,
        size: int,
        tolerance: list,
        from_node: list,
        to_node: list,
        distance: list,
        edge: int,
        rate: list,
        upper_limit: list,
        production_rate: list,
        iter: int,
        verbosity: int=0) -> Result:
    """[summary]

    Args:
        time_end (float): [description]
        seed (int): [description]
        num_vehicles (int): [description]
        vehicle_cost (float): [description]
        unit_cost (list): [description]
        unit_revenue (list): [description]
        size (int): [description]
        tolerance (list): [description]
        from_node (list): [description]
        to_node (list): [description]
        distance (list): [description]
        edge (int): [description]
        rate (list): [description]
        upper_limit (list): [description]
        production_rate (list): [description]
        iter (int): [description]
        verbosity (int, optional): [description]. Defaults to 0.

    Returns:
        Result: [description]
    """
    profit = [0 for i in range(iter)]
    num_orders = [0 for i in range(iter)]
    num_delivered = [0 for i in range(iter)]
    deliverysimulator._base.run(time_end,
                                seed,
                                num_vehicles,
                                vehicle_cost,
                                unit_cost,
                                unit_revenue,
                                size,
                                tolerance,
                                from_node,
                                to_node,
                                distance,
                                edge,
                                rate,
                                upper_limit,
                                production_rate,
                                iter,
                                verbosity,
                                profit,
                                num_orders,
                                num_delivered)
    return Result(profit, num_orders, num_delivered)
