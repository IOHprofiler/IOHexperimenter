Publishing experiment archives
==============================

The :meth:`ioh.Experiment.publish` helper streamlines turning an experiment run
into a shareable Zenodo deposit. After running an experiment, calling
``publish`` will:

* generate the ``ioh_data.zip`` archive with an up-to-date ``README.md``
  describing the algorithm, problems, instances, dimensions, repetitions and
  optional evaluation budget;
* serialise the algorithm instance with :mod:`cloudpickle` so it can be
  restored when reproducing the experiment; and
* upload the archive, README and algorithm bundle to Zenodo using their REST
  API.

> [!IMPORTANT] You need a Zenodo access token with appropriate permissions to upload
> deposits. You can create and manage your tokens in your Zenodo account
> settings. See https://zenodo.org/account/settings/applications/tokens/new/

Example usage::

    exp = ioh.Experiment(
        my_algorithm,
        fids=[1, 2, 3],
        iids=[1],
        dims=[5, 10],
        reps=5,
    )
    exp()  # run the benchmark
    exp.publish(
        zenodo_token="<your access token>",
        title="Benchmarking IOH on BBOB",
        description="Log data for the benchmark run.",
        creators=[{"name": "Doe, Jane"}],
        budget=20000,
        keywords=["optimization", "benchmark"],
        sandbox=True,
    )

By default the helper targets the production Zenodo endpoint. Pass
``sandbox=True`` while testing credentials or automation against the sandbox
service. You can also provide an existing :class:`requests.Session` to reuse
connections or configure proxies, and the ``additional_metadata`` parameter to
add any extra Zenodo metadata fields.

