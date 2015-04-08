<?php
/* @var $this WSNtableController */
/* @var $dataProvider CActiveDataProvider */

$this->breadcrumbs=array(
	'Wsntables',
);

$this->menu=array(
	array('label'=>'Create WSNtable', 'url'=>array('create')),
	array('label'=>'Manage WSNtable', 'url'=>array('admin')),
);
?>

<h1>Wsntables</h1>

<?php $this->widget('zii.widgets.CListView', array(
	'dataProvider'=>$dataProvider,
	'itemView'=>'_view',
)); ?>
