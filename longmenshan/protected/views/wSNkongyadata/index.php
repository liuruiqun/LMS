<?php
/* @var $this WSNkongyadataController */
/* @var $dataProvider CActiveDataProvider */

$this->breadcrumbs=array(
	'Wsnkongyadatas',
);

$this->menu=array(
	array('label'=>'Create WSNkongyadata', 'url'=>array('create')),
	array('label'=>'Manage WSNkongyadata', 'url'=>array('admin')),
);
?>

<h1>Wsnkongyadatas</h1>

<?php $this->widget('zii.widgets.CListView', array(
	'dataProvider'=>$dataProvider,
	'itemView'=>'_view',
)); ?>
