# syn editor #
* editor for _adapter-sequence-user data_ (asu) files
* support for custom adapters as plugins
* support for import/export sequences and user data

## adapters ##
* specify supported commands
* create target objects
* build execution environment, create and set up devices, resources (openGL, sound device,etc.)
* provide UI components (dialogs, LOVs, etc) for the editor

## sequences ##
* store timelines of commands
* rendered using the _MultiSeriesView_
* operations
  * import from different formats
  * export
  * split and merge

## user data blocks ##
* store data used by objects via adapters
* created
 * import from different formats
 * with editor extensions provided by the adapters
