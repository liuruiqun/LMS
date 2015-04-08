<?php
/* @var $this WSNtableController */
/* @var $model WSNtable */

$this->breadcrumbs=array(
	'Wsntables'=>array('index'),
	'Create',
);

$this->menu=array(
	array('label'=>'List WSNtable', 'url'=>array('index')),
	array('label'=>'Manage WSNtable', 'url'=>array('admin')),
);
?>

<h1>Create WSNtable</h1>

<?php $this->renderPartial('_form', array('model'=>$model)); ?>