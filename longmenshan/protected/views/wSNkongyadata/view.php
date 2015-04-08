<?php
/* @var $this WSNkongyadataController */
/* @var $model WSNkongyadata */

$this->breadcrumbs=array(
	'Wsnkongyadatas'=>array('index'),
	$model->DID,
);

$this->menu=array(
	array('label'=>'List WSNkongyadata', 'url'=>array('index')),
	array('label'=>'Create WSNkongyadata', 'url'=>array('create')),
	array('label'=>'Update WSNkongyadata', 'url'=>array('update', 'id'=>$model->DID)),
	array('label'=>'Delete WSNkongyadata', 'url'=>'#', 'linkOptions'=>array('submit'=>array('delete','id'=>$model->DID),'confirm'=>'Are you sure you want to delete this item?')),
	array('label'=>'Manage WSNkongyadata', 'url'=>array('admin')),
);
?>

<h1>View WSNkongyadata #<?php echo $model->DID; ?></h1>

<?php $this->widget('zii.widgets.CDetailView', array(
	'data'=>$model,
	'attributes'=>array(
		'DID',
		'SID',
		'DataTime',
		'S1',
		'S2',
		'R1',
		'R2',
		'IsWarning',
	),
)); ?>
