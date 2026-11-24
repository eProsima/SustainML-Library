.. _user_manual_node_status:

Node status and saved data
==========================

The *Node status* screen shows the state of the framework nodes and manages the saved data.
Open it with the activity icon (a pulse line) at the right end of the header of every screen.
The icon is disabled while a task is running.

.. _user_manual_node_status_nodes:

Framework nodes
---------------

The **Framework nodes** card lists the six nodes with their current state, updated every half second:
**App Requirements**, **Carbon Footprint**, **HW Constraints**, **HW Resources**, **ML Model** and **Model Metadata**.

.. list-table::
    :header-rows: 1
    :widths: 25 75

    * - State
      - Meaning
    * - **Idle**
      - The node is ready and waiting for a task.
    * - **Running**
      - The node is working on a task.
    * - **Initializing**
      - The node is starting.
    * - **Inactive**
      - The node has not reported any state (for example, it is not running).
    * - **Error**
      - The node reported an error.
    * - **Terminating**
      - The node is shutting down.

The application is ready to submit problems when all six nodes are **Idle**.

.. figure:: /rst/figures/user_manual/node_status.png
    :align: center
    :alt: Node status screen with the six framework nodes Idle

    The *Node status* screen, with every node ready.

.. _user_manual_node_status_save_load:

Saving and loading
------------------

Results and Hugging Face history live only while the application is open.
To keep them, save them into a *named save file*, and load it in a later session.
The same save files are used everywhere, and each one can hold three parts:
the results of the problems, the Hugging Face searches and the Hugging Face comparisons.

.. list-table::
    :header-rows: 1
    :widths: 30 35 35

    * - Where
      - **Save** stores
      - **Load** adds
    * - *Node status* screen
      - The results of the open problems, the HF searches and the HF comparisons.
      - All three parts of the file.
    * - *Results* screen
      - The results of the open problems.
      - The results (only files that have results are listed).
    * - *Search results* screen
      - The HF searches.
      - The HF searches (only files that have searches are listed).
    * - *Model comparison* screen
      - The HF comparisons.
      - The HF comparisons (only files that have comparisons are listed).

Saving
^^^^^^

#. Click **Save**.
#. Type a name for the save (for example, *experiment 1*). Letters, digits, spaces, ``_``, ``-`` and ``/`` are allowed.
   To replace an existing save, click its name in the **Existing saves** list instead.
#. Click **Save**.

.. figure:: /rst/figures/user_manual/save_dialog.png
    :align: center
    :alt: Save Results dialog with a new name typed and one existing save listed

    Saving the results of the example as *support emails*, next to an existing save.

Saving into an existing file replaces only the parts that screen saves, and keeps the others.
For example, saving from the *Search results* screen replaces the searches of the file but keeps its results and
comparisons.

The **Save** button of the *Node status* screen is disabled when the screen is opened from the start screen,
as there is nothing to save yet.
The **Save** button of the *Results* screen is enabled once a problem has the results of all six nodes,
and those of the *Search results* and *Model comparison* screens once there is a search or a comparison.

Loading
^^^^^^^

#. Click **Load**.
#. Click a saved file in the list. **No saved files yet.** is shown when there is none.
#. Click **Load**.

.. figure:: /rst/figures/user_manual/load_dialog.png
    :align: center
    :alt: Load Results dialog with two saves, the first one selected

    The **Load** dialog, with a save selected and its trash icon on the right.

Loading never replaces what is open:

* Each loaded problem opens in a new tab of the *Results* screen, with its saved results and tab name.
* Loaded searches and comparisons are added to the current ones. Up to ten comparisons are kept.

To delete a single save file, click the trash icon next to its name in the **Load** dialog.
The file is deleted at once, without confirmation.

Clearing saved data
^^^^^^^^^^^^^^^^^^^

**Clear saved data** on the *Node status* screen deletes every save file, after a confirmation.
Tasks in progress are not affected.

Where save files are stored
^^^^^^^^^^^^^^^^^^^^^^^^^^^

The back-end node keeps one SQLite file (``.sqlite3``) per save, in the first of these folders that can be used:

#. The folder set in the ``SUSTAINML_DB_PATH`` environment variable, if set.
#. ``/opt/sustainml/db``.
#. ``sustainml`` in the user data folder (``$XDG_DATA_HOME``, by default ``~/.local/share``).
