<?php
/* @var $this WSNkongyadataController */
/* @var $model WSNkongyadata */

$this->breadcrumbs=array(
	'Wsnkongyadatas'=>array('index'),
	$model->DID=>array('view','id'=>$model->DID),
	'Update',
);

$this->menu=array(
	array('label'=>'List WSNkongyadata', 'url'=>array('index')),
	array('label'=>'Create WSNkongyadata', 'url'=>array('create')),
	array('label'=>'View WSNkongyadata', 'url'=>array('view', 'id'=>$model->DID)),
	array('label'=>'Manage WSNkongyadata', 'url'=>array('admin')),
);
?>

<h1>Update WSNkongyadata <?php echo $model->DID; ?></h1>

<?php $this->renderPartial('_form', array('model'=>$model)); ?>